#!/bin/bash
codecs=codecs.cfg
rates=rates.cfg
sequences=sequences.cfg
cmd_template=EvalCodecStepResponse.template

# iterate over all codecs, rates, and sequences

while read name codec codec_mt file_ext decoder dec_if codec_parameters
do
#  echo "Codec: $name $codec $codec_mt $decoder"
  if [[ $name = \#* ]]; then
    continue
  fi

  while read rate_mode switch_mode rate_descriptor
  do
#    echo "rates: $rate_mode $switch_mode $rate_descriptor"
    if [[ $rate_mode = \#* ]]; then
      continue
    fi

    while read sequence width height fps total_frames
    do
      if [[ $sequence = \#* ]]; then
        continue
      fi

      echo "Codec: $name $codec $codec_mt $decoder sequence : $sequence $width"x"$height@$fps rate_mode: $rate_mode switch_mode: $switch_mode rate_descriptor: $rate_descriptor"

      esc_seq=$(echo $sequence | sed 's/\//\\\//g')
      fn=$(basename $sequence)
      base_fn=${fn%.*}
      id=$base_fn"_"$width"_"$height"_"$fps"_"$name
# for each of the rates, generate script and ref
      rate_params=(${rate_descriptor//,/ })
      for rate in "${rate_params[@]}"
      do
        echo "Rate: $rate"
        pair=(${rate//:/ })
        current_rate=${pair[0]}
        current_duration=${pair[1]}
        rd="$current_rate:-1"
        id_string="$id"_"$current_rate"
        out="enc_$id_string"
	script="gen_ref_$id_string.sh"
        yuv_file="$out".yuv
        csv_file="$out.psnr.csv"
        sum_csv=references/"sum_$out".csv
	psnr_file=references/"$out".psnr.csv
	bpp_ref_file=references/"$out".bpp.csv
	bpp_temp_ref_file=references/_"$out".bpp.csv

# generate script cmd line
########################## reference 1 ##########################################
        if [ ! -f $psnr_file ]; then
          echo "Referencei $psnr_file does not exist - generating"
          sed -e "s/<<codec>>/$codec/g" EvalCodecStepResponse.template > $script
          sed -i -e "s/<<codec_mt>>/$codec_mt/g" $script
          sed -i -e "s/<<sequence>>/$esc_seq/g" $script
          sed -i -e "s/<<width>>/$width/g" $script
          sed -i -e "s/<<height>>/$height/g" $script
          sed -i -e "s/<<fps>>/$fps/g" $script
          sed -i -e "s/<<out>>/$out/g" $script
          sed -i -e "s/<<switch_mode>>/$switch_mode/g" $script
          sed -i -e "s/<<rate_mode>>/$rate_mode/g" $script
          sed -i -e "s/<<rate_descriptor>>/$rd/g" $script
# generate additional params
       	  additional_params=""
     	  codec_params=(${codec_parameters//;/ })
          for param in "${codec_params[@]}"
          do
            echo "Codec param: $param"
            # or do whatever with individual element of the array
            p=$(sed -e "s/<<vc_param>>/$param/g" vc_param.template)
     	    additional_params="$additional_params$p"
          done
          echo "add: $additional_params"
          sed -i -e "s/<<additional_params>>/$additional_params/g" $script
   
          chmod 755 $script
# run cmd line
          echo "Generating reference "
          bash $script
          res=$?
          if [ $res -ne 0 ]; then
            echo "Error running script: $script"
            exit -1
          fi 

          echo "Frame Time NALUs Bpp TargetBpp Size" > $csv_file
          grep "ECSR #1" logs/EvalCodecStepResponse.INFO | awk '{print $8" "$10" "$12" "$14" "$17" "$21" "}' >> "$csv_file"
# decode encoded file for PSNR
          echo "Decoding and calculating PSNR"
          ../$decoder -"$dec_if" "$out"."$file_ext" -o "$yuv_file"  > "$out".dec.txt
  	
          echo "Generating PSNR: " $psnr_file
          echo "Frame PSNR_Y PSNR_U PSNR_V" > $psnr_file
          ../GeneratePSNR $width $height $sequence "$yuv_file" >> $psnr_file
          sed -i "s/,/./g" $psnr_file
# delete yuv file for space constraints
          echo "Deleting YUV"
  	  rm $yuv_file
# get rid of total's and empty line
          lines=$(wc -l "$psnr_file" | awk '{print $1}')
          new_lines=$(($lines - 3))
     	  head -n"$new_lines" "$psnr_file" > "$psnr_file".tmp
          mv "$psnr_file".tmp $psnr_file
# merge files
          paste $csv_file $psnr_file | awk '{print $1" "$2" "$3" "$4" "$5" "$6" "$8}' > $sum_csv
        fi
      done
    done < $sequences
  done < $rates
done < $codecs


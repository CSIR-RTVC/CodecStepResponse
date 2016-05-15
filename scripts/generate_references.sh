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
        script="gen_ref_$id.sh"
	script1="gen_ref_$id1.sh"
	script2="gen_ref_$id2.sh"
# for each of the rates, generate script and ref
        rate_params=(${rate_descriptor//,/ })
        first=${rate_params[0]}
        pair1=(${first//:/ })
        rate1=${pair1[0]}
        rd1="$rate1:-1"
	echo "Rate 1: ${pair1[0]} Duration: ${pair1[1]}"        
	second=${rate_params[1]}
        pair2=(${second//:/ })
	rate2=${pair2[0]}
        rd2="$rate2:-1"
        echo "Rate 2: ${pair2[0]} Duration: ${pair2[1]}"
	
	id1=$id"_"$rate1
        id2=$id"_"$rate2

        out1="enc_$id1"
        out2="enc_$id2"
        yuv_file1="$out1".yuv
        yuv_file2="$out2".yuv
        csv_file1="$out1.psnr.csv"
        csv_file2="$out2.psnr.csv"
        sum_csv1=references/"sum_$out1".csv
        sum_csv2=references/"sum_$out2".csv
	psnr_file1=references/"$out1".psnr.csv
	psnr_file2=references/"$out2".psnr.csv
	
	bpp_ref_file1=references/"$out1".bpp.csv
	bpp_temp_ref_file1=references/_"$out1".bpp.csv
	bpp_ref_file2=references/"$out2".bpp.csv
	bpp_temp_ref_file2=references/_"$out2".bpp.csv

# generate script cmd line
########################## reference 1 ##########################################
        if [ ! -f $psnr_file1 ]; then
          echo "Reference 1 does not exist - generating"
          sed -e "s/<<codec>>/$codec/g" EvalCodecStepResponse.template > $script1
          sed -i -e "s/<<codec_mt>>/$codec_mt/g" $script1
          sed -i -e "s/<<sequence>>/$esc_seq/g" $script1
          sed -i -e "s/<<width>>/$width/g" $script1
          sed -i -e "s/<<height>>/$height/g" $script1
          sed -i -e "s/<<fps>>/$fps/g" $script1
#          sed -i -e "s/<<mode>>/$mode/g" $script1
          sed -i -e "s/<<out>>/$out1/g" $script1
          sed -i -e "s/<<switch_mode>>/$switch_mode/g" $script1
          sed -i -e "s/<<rate_mode>>/$rate_mode/g" $script1
          sed -i -e "s/<<rate_descriptor>>/$rd1/g" $script1
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
          sed -i -e "s/<<additional_params>>/$additional_params/g" $script1

          chmod 755 $script1
# run cmd line
          echo "Generating reference 1"
          bash $script1
          res=$?
          if [ $res -ne 0 ]; then
            echo "Error running script: $script1"
            exit -1
          fi 

          echo "Frame Time NALUs Bpp TargetBpp Size" > $csv_file1
          grep "ECSR #1" logs/EvalCodecStepResponse.INFO | awk '{print $8" "$10" "$12" "$14" "$17" "$21" "}' >> "$csv_file1"
# decode encoded file for PSNR
          echo "Decoding and calculating PSNR"
          ../$decoder -"$dec_if" "$out1"."$file_ext" -o "$yuv_file1"  > "$out1".dec.txt
  	
          echo "Generating PSNR: " $psnr_file1
          echo "Frame PSNR_Y PSNR_U PSNR_V" > $psnr_file1
          ../GeneratePSNR $width $height $sequence "$yuv_file1" >> $psnr_file1
          sed -i "s/,/./g" $psnr_file1
# delete yuv file for space constraints
          echo "Deleting YUV1"
  	  rm $yuv_file1
# get rid of total's and empty line
          lines=$(wc -l "$psnr_file1" | awk '{print $1}')
    	  new_lines=$(($lines - 3))
  	  head -n"$new_lines" "$psnr_file1" > "$psnr_file1".tmp
          mv "$psnr_file1".tmp $psnr_file1
# merge files
          paste $csv_file1 $psnr_file1 | awk '{print $1" "$2" "$3" "$4" "$5" "$6" "$8}' > $sum_csv1

        fi

########################## reference 2 ##########################################
        if [ ! -f $psnr_file2 ]; then
          echo "Reference 2 does not exist - generating"
          sed -e "s/<<codec>>/$codec/g" EvalCodecStepResponse.template > $script2
          sed -i -e "s/<<codec_mt>>/$codec_mt/g" $script2
          sed -i -e "s/<<sequence>>/$esc_seq/g" $script2
          sed -i -e "s/<<width>>/$width/g" $script2
          sed -i -e "s/<<height>>/$height/g" $script2
          sed -i -e "s/<<fps>>/$fps/g" $script2
#          sed -i -e "s/<<mode>>/$mode/g" $script2
          sed -i -e "s/<<out>>/$out2/g" $script2
          sed -i -e "s/<<switch_mode>>/$switch_mode/g" $script2
          sed -i -e "s/<<rate_mode>>/$rate_mode/g" $script2
          sed -i -e "s/<<rate_descriptor>>/$rd2/g" $script2
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
          sed -i -e "s/<<additional_params>>/$additional_params/g" $script2

          chmod 755 $script2
# run cmd line
          echo "Generating reference 2"
          bash $script2
          res=$?
          if [ $res -ne 0 ]; then
            echo "Error running script: $script2"
            exit -1
          fi 

          echo "Frame Time NALUs Bpp TargetBpp Size" > $csv_file2
          grep "ECSR #1" logs/EvalCodecStepResponse.INFO | awk '{print $8" "$10" "$12" "$14" "$17" "$21" "}' >> "$csv_file2"
# decode encoded file for PSNR
          echo "Decoding and calculating PSNR"
          ../$decoder -"$dec_if" "$out2"."$file_ext" -o "$yuv_file2"  > "$out2".dec.txt
  	
          echo "Generating PSNR: " $psnr_file2
          echo "Frame PSNR_Y PSNR_U PSNR_V" > $psnr_file2
          ../GeneratePSNR $width $height $sequence "$yuv_file2" >> $psnr_file2
          sed -i "s/,/./g" $psnr_file2
# delete yuv file for space constraints
          echo "Deleting YUV2"
  	  rm $yuv_file2
# get rid of total's and empty line
          lines=$(wc -l "$psnr_file2" | awk '{print $1}')
    	  new_lines=$(($lines - 3))
  	  head -n"$new_lines" "$psnr_file2" > "$psnr_file2".tmp
          mv "$psnr_file2".tmp $psnr_file2

# merge files
          paste $csv_file2 $psnr_file2 | awk '{print $1" "$2" "$3" "$4" "$5" "$6" "$8}' > $sum_csv2
        fi

      done < $sequences

  done < $rates

done < $codecs


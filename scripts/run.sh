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
        script="eval_$id.sh"
        rscript="eval_$id.R"
        out="enc_$id"
 
# for each of the rates, generate script and ref
        rate_params=(${rate_descriptor//,/ })
	id_string=""
        for rate in "${rate_params[@]}"
        do
           echo "Rate: $rate"
           pair=(${rate//:/ })
           current_rate=${pair[0]}
           current_duration=${pair[1]}
           id_string="$id_string"_"$current_rate"_"$current_duration"
        done
	out="$out$id_string"
        echo "Unique id_string: $id_string Updated out: $out"
        first=${rate_params[0]}
        pair1=(${first//:/ })
        rate1=${pair1[0]}
	echo "Rate 1: ${pair1[0]} Duration: ${pair1[1]}"        
	second=${rate_params[1]}
        pair2=(${second//:/ })
	rate2=${pair2[0]}
        echo "Rate 2: ${pair2[0]} Duration: ${pair2[1]}"

	id1=$id"_"$rate1
        id2=$id"_"$rate2
        out1="enc_$id1"
        out2="enc_$id2"
        sum_csv1="references\/sum_$out1".csv
        sum_csv2="references\/sum_$out2".csv

	ref_id1=$id"_"$rate1
        ref_id2=$id"_"$rate2
	psnr_file1="references\/enc_""$ref_id1".psnr.csv
	psnr_file2="references\/enc_""$ref_id2".psnr.csv
	bpp_ref_file1="references\/enc_""$ref_id1".bpp.csv
	bpp_ref_file2="references\/enc_""$ref_id2".bpp.csv

	echo "Ref1: $ref_id1 Ref2: $ref_id2"

# generate sender cmd line
        sed -e "s/<<codec>>/$codec/g" EvalCodecStepResponse.template > $script
        sed -i -e "s/<<codec_mt>>/$codec_mt/g" $script
        sed -i -e "s/<<sequence>>/$esc_seq/g" $script
        sed -i -e "s/<<width>>/$width/g" $script
        sed -i -e "s/<<height>>/$height/g" $script
        sed -i -e "s/<<fps>>/$fps/g" $script
        #sed -i -e "s/<<mode>>/$mode/g" $script
        sed -i -e "s/<<out>>/$out/g" $script
        sed -i -e "s/<<switch_mode>>/$switch_mode/g" $script
        sed -i -e "s/<<rate_mode>>/$rate_mode/g" $script
        sed -i -e "s/<<rate_descriptor>>/$rate_descriptor/g" $script

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
        echo "Running script"
        bash $script
        
        res=$?
        if [ $res -ne 0 ]; then
          echo "Error running script: $script"
          exit -1
        fi 
	csv_file="$out".csv
        echo "Frame Time NALUs Bpp TargetBpp Size" > $csv_file
        grep "ECSR #1" logs/EvalCodecStepResponse.INFO | awk '{print $8" "$10" "$12" "$14" "$17" "$21" "}' >> "$csv_file"
        yuv_file="$out".yuv

## decode encoded file for PSNR
        echo "Decoding and calculating PSNR"
        ../$decoder -"$dec_if" "$out"."$file_ext" -o "$yuv_file"  > "$out".dec.txt

	psnr_file="$out".psnr.csv
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
        echo "new lines in file $new_lines."
	head -n"$new_lines" "$psnr_file" > "$psnr_file".tmp
        mv "$psnr_file".tmp $psnr_file

# merge files
        sum_csv="sum_$out".csv
        paste $csv_file $psnr_file | awk '{print $1" "$2" "$3" "$4" "$5" "$6" "$8}' > $sum_csv
# replace spaces with commas
        sed -i "s/ /,/g" $sum_csv
        
#        exit 0
# generate R script
	echo "Generating R scripts"
        sed -e "s/<<file>>/$sum_csv/g" plot.R.template > $rscript
        sed -i -e "s/<<width>>/$width/g" $rscript
        sed -i -e "s/<<height>>/$height/g" $rscript
        sed -i -e "s/<<fps>>/$fps/g" $rscript
        sed -i -e "s/<<total_frames>>/$total_frames/g" $rscript
        sed -i -e "s/<<switch_mode>>/$switch_mode/g" $rscript
        sed -i -e "s/<<rate_mode>>/$rate_mode/g" $rscript
        sed -i -e "s/<<rate_descriptor>>/$rate_descriptor/g" $rscript
        sed -i -e "s/<<psnr_file1>>/$psnr_file1/g" $rscript
        sed -i -e "s/<<psnr_file2>>/$psnr_file2/g" $rscript
        sed -i -e "s/<<bpp_ref_file1>>/$bpp_ref_file1/g" $rscript
        sed -i -e "s/<<bpp_ref_file2>>/$bpp_ref_file2/g" $rscript
        sed -i -e "s/<<sum_csv1>>/$sum_csv1/g" $rscript
        sed -i -e "s/<<sum_csv2>>/$sum_csv2/g" $rscript

# run R script
        echo "Running R script"
	Rscript $rscript

      done < $sequences

  done < $rates

#./TAppDecoderStatic -b out_test.265 -o out_test.yuv
#./ldecod.exe -i out_test.264 -o out_test.yuv

done < $codecs


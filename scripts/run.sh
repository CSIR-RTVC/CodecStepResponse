#!/bin/bash

codecs=codecs.cfg
rates=rates.cfg
sequences=sequences.cfg
cmd_template=EvalCodecStepResponse.template

# iterate over all codecs, bitrates, and switches and sequences

while read name codec codec_mt file_ext mode decoder dec_if codec_parameters
do
#  echo "Codec: $name $codec $codec_mt $mode $decoder"
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

        echo "Codec: $name $codec $codec_mt $mode $decoder $rcmt $cbrf sequence : $sequence $width"x"$height@$fps rate_mode: $rate_mode switch_mode: $switch_mode rate_descriptor: $rate_descriptor"

        esc_seq=$(echo $sequence | sed 's/\//\\\//g')
        fn=$(basename $sequence)
	base_fn=${fn%.*}
        id=$base_fn"_"$width"_"$height"_"$fps"_"$name
        script="eval_$id.sh"
        rscript="eval_$id.R"
        out="enc_$id"
        
	ref_id1=$base_fn"_"$width"_"$height"_"$fps"_"$name
        ref_id2=$base_fn"_"$width"_"$height"_"$fps"_"$name
	psnr_file1="references\/enc_""$ref_id1".psnr.csv
	psnr_file2="references\/enc_""$ref_id2".psnr.csv
	bpp_ref_file1="references\/enc_""$ref_id1".bpp.csv
	bpp_ref_file2="references\/enc_""$ref_id2".bpp.csv

# generate sender cmd line
        sed -e "s/<<codec>>/$codec/g" EvalCodecStepResponse.template > $script
        sed -i -e "s/<<codec_mt>>/$codec_mt/g" $script
        sed -i -e "s/<<sequence>>/$esc_seq/g" $script
        sed -i -e "s/<<width>>/$width/g" $script
        sed -i -e "s/<<height>>/$height/g" $script
        sed -i -e "s/<<fps>>/$fps/g" $script
        sed -i -e "s/<<mode>>/$mode/g" $script
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
#        exit 0
        
        res=$?
        if [ $res -ne 0 ]; then
          echo "Error running script: $script"
          exit -1
        fi 
## generate analyser file
#        echo "Analysing encoded file"
#        GLOG_v=5 GLOG_logtostderr=1 ../VideoAnalyser -s "$out"."$file_ext" --fps "$fps" --format "$codec_mt" -v 2> "$out".txt
#
## extract sizes into csv
#        echo "Extracting NALU info"
#        grep "Video AU" "$out".txt | awk '{print $7" "$10" "$12" "substr($15,2,length($15)-2)" "substr($18,2,length($18)-2)" "substr($21,2,length($21)-2)" "substr($24,2,length($24)-2)" "substr($27,2,length($27)-2)" "}' > "$out".csv
#
#        exit 0
	csv_file="$out".csv
        echo "Frame Time NALUs Bpp TargetBpp Size" > $csv_file
        grep "ECSR #1" logs/EvalCodecStepResponse.INFO | awk '{print $8" "$10" "$12" "$14" "$17" "$21" "}' >> "$csv_file"
## sum NALU sizes into CSV file with sums
#	csv_file2=_"$out".csv
        yuv_file="$out".yuv
#
#        echo "Frame Time NALUs Size Bpp" > $csv_file2
#        while read frame_num time count size1 size2 size3 size4 size5 size6
#        do
#          sum=0
#          sum=$(($sum + $size1))
#          if [[ ! -z $size2 ]]
#          then
#            sum=$(($sum + $size2))
#          fi
#          if [[ ! -z $size3 ]]
#          then
#            sum=$(($sum + $size3))
#          fi
#          if [[ ! -z $size4 ]]
#          then
#            sum=$(($sum + $size4))
#          fi
#          if [[ ! -z $size5 ]]
#          then
#            sum=$(($sum + $size5))
#          fi
#          if [[ ! -z $size6 ]]
#          then
#            sum=$(($sum + $size6))
#          fi
## calculate bits per pixel
#          bpp=$(bc -l <<< "scale=8; ($sum*8)/($width*$height)")
#	  bpp2=$(echo $bpp | sed -r 's/^\./0./')
#
#          echo "$frame_num $time $count $sum $bpp2" >> $csv_file2
#        done < $csv_file
## overwrite file with separate NALUs
#	mv $csv_file2 $csv_file
#        
##	exit 0
## decode encoded file for PSNR
        echo "Decoding and calculating PSNR"
        ../$decoder -"$dec_if" "$out"."$file_ext" -o "$yuv_file"  > "$out".dec.txt

#	exit 0
	
	psnr_file="$out".psnr.csv
        echo "Generating PSNR: " $psnr_file
        echo "Frame PSNR_Y PSNR_U PSNR_V" > $psnr_file
        ../GeneratePSNR $width $height $sequence "$yuv_file" >> $psnr_file
        sed -i "s/,/./g" $psnr_file
# delete yuv file for space constraints
        echo "Deleting YUV"
	rm $yuv_file

#	exit 0
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

# run R script
        echo "Running R script"
	Rscript $rscript

#        exit 0
      done < $sequences

  done < $rates

#./TAppDecoderStatic -b out_test.265 -o out_test.yuv
#./ldecod.exe -i out_test.264 -o out_test.yuv

done < $codecs





echo "vvvv Top Of Binary" 
xxd -l 128 ./build/NucleoBaseExample-signed-confirmed.bin
echo " " 
xxd -s -128 ./build/NucleoBaseExample-signed-confirmed.bin
echo "^^^ Bottom Of Binary" 

python3 ../mcuboot/scripts/imgtool.py dumpinfo ./build/NucleoBaseExample-signed-confirmed.bin 
python3 ../mcuboot/scripts/imgtool.py verify build/NucleoBaseExample-signed-confirmed.bin

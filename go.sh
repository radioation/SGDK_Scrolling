 
FILES=$(find . -name main.c)

for FILE in ${FILES}
do
  sed -i -e 's/fix32ToInt/F32_toInt/g' $FILE 
  sed -i -e 's/fix32ToInt/F32_toInt/g' $FILE
  sed -i -e 's/fix32Div/F32_div/g' $FILE 
  sed -i -e 's/fix32Mul/F32_mul/g' $FILE 
  sed -i -e 's/fix16ToInt/F16_toInt/g' $FILE 
  sed -i -e 's/fix16Mul/F16_mul/g' $FILE 
  sed -i -e 's/fix16Div/F16_div/g' $FILE 
  sed -i -e 's/intToFix16/FIX16/g' $FILE 
done

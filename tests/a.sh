mkdir assembly
for file in *.sy
do
  cp "$file" "assembly/$(basename "$file" .sy).c";
done

cd assembly
for file in *.c
do
  gcc -S "$file";
  rm "$file";
done
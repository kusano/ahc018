set -e

g++ -O2 -DTOPCODER_LOCAL A.cpp -o A
for i in $(seq 0 99)
do
  f=$(printf %04d.txt ${i})
  echo -n "${f} "
  ./A < tools/in/${f} > output/${f}
done

set -e

g++ -O2 A.cpp -o A
for i in $(seq 0 99)
do
  f=$(printf %04d.txt ${i})
  echo ${f}
  cargo run --manifest-path tools/Cargo.toml --release --bin tester ./A < tools/in/${f} > output/${f}
done

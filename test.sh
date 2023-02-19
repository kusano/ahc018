set -e

g++ -O2 A.cpp -o A
for i in $(seq 0 99)
do
  f=$(printf %04d.txt ${i})
  # cargo run --manifest-path tools/Cargo.toml --release --bin tester ./A < tools/in/${f} > output/${f}
  echo -n "${f} "
  ./A < tools/in/${f} > output/${f}
done
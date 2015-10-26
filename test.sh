sudo ./load.sh

echo "1" > /dev/fibonacci_input
result=$(cat /dev/fibonacci_result)
input=$(cat /dev/fibonacci_input)
echo "$input: $result"
echo ""

echo "2" > /dev/fibonacci_input
result=$(cat /dev/fibonacci_result)
input=$(cat /dev/fibonacci_input)
echo "$input: $result"
echo ""

echo "5" > /dev/fibonacci_input
result=$(cat /dev/fibonacci_result)
input=$(cat /dev/fibonacci_input)
echo "$input: $result"
echo ""

echo "20" > /dev/fibonacci_input
result=$(cat /dev/fibonacci_result)
input=$(cat /dev/fibonacci_input)
echo "$input: $result"
echo ""


sudo ./unload.sh

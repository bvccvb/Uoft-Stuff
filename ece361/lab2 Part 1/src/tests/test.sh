#!/bin/bash

RAND_PORT=$((RANDOM + 32767))

### RUN SERVER
../server RAND_PORT &
SERVER_PID=$!

sleep 1

### CLIENT TESTS
client_test_1()
{
    echo "/invalid_command"
    echo "invalid session"
}
client_test_2()
{
    echo "/login alice alpine 0.0.0.0 0"
}
client_test_3()
{
    echo "/login alice alpine localhost $RAND_PORT"
}

### RUN CLIENT
echo "### TEST 1 ###"
client_test_1 | ../client
CLIENT_PID=$!
echo

echo "### TEST 2 ###"
client_test_2 | ../client
CLIENT_PID=$!
echo

echo "### TEST 3 ###"
client_test_3
client_test_3 | ../client
CLIENT_PID=$!
echo

### CLEANUP
echo

kill -9 $SERVER_PID

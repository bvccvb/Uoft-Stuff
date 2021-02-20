clc;clear all;

[sequence_number_s, send_time] = textread('send.txt', '%f %f');
[sequence_number_r, receive_time] = textread('rec.txt', '%f %f');

receive_time  = receive_time - send_time(1);
send_time = send_time - send_time(1);
receive_time = receive_time/1000;
send_time = send_time/1000;

figure(1)
plot(sequence_number_s, send_time);
hold on

plot(sequence_number_r, receive_time);
title('Exercise 1.5 -Evaluation');
ylabel('Time (Microeconds)');
xlabel('Sequence Number');

legend('Send Times', 'Receive Times' )

hold off

figure (2)
plot(sequence_number_s, receive_time - send_time);

packet_size = 4;
bytes = ones(size(sequence_number_s)) .* packet_size;


figure (3)
hold on
plot(receive_time, cumsum(bytes));
plot(send_time, cumsum(bytes));


            


clc;clear all;

[time, byte] = textread('receiver.txt', '%f %f');
[time5, byte2, backlog, content] = textread('bucket.txt', '%f %f %f %f');


time4 = time/1000;
time3 = cumsum(time4);

time6 = time5/1000;
time7 = cumsum(time6);
cumulative2 = cumsum(byte);
cumulative3 = cumsum(byte2);
cumulative4 = cumsum(backlog);

figure(1)

plot(time3, cumulative2)
title('Exercise 3.1 -(Long-term) Bandwidth is cheap');
ylabel('Frame Size (Bytes)');
xlabel('Time (Miliseconds)');

hold on

plot(time7, cumulative3);
legend('Output File (Sink)', 'Token Bucket' )

hold off



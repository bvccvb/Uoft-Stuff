clc;clear all;

[time_p, packetsize_p] = textread('T100N5L10.data', '%f %f');
[time, byte] = textread('receiver.txt', '%f %f');

time2 = time_p - time_p(1);
time2 = time2 * 1000;
time3 = cumsum(time);
cumulative1 = cumsum(packetsize_p);
cumulative2 = cumsum(byte);

figure(1)
plot(time2, cumulative1)
title('Lab 2 : Exercise 2.3 Improved Traffic Generator');
ylabel('Frame Size (Bytes)');
xlabel('Time (Microseconds)');

hold on

plot(time3, cumulative2)
legend('Data from data','receiver.txt')

hold off



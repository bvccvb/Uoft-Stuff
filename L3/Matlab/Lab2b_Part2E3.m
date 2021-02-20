clc;clear all;

[time_p, packetsize_p] = textread('T1N30L325.data', '%f %f');
[time, byte] = textread('receiver.txt', '%f %f');
[time5, byte2, backlog, content] = textread('bucket.txt', '%f %f %f %f');


time2 = time_p - time_p(1);
time2 = time2;
time4 = time/1000;
time3 = cumsum(time4);

time6 = time5/1000;
time7 = cumsum(time6);
cumulative1 = cumsum(packetsize_p);
cumulative2 = cumsum(byte);
cumulative3 = cumsum(byte2);
cumulative4 = cumsum(backlog);

figure(1)
plot(time2, cumulative1)
title('Exercise 2.4 - Maximum rate of Token Bucket');
ylabel('Frame Size (Bytes)');
xlabel('Time (Miliseconds)');

hold on

plot(time3, cumulative2)
plot(time7, cumulative3);
legend('Trace Data(Traffic Generator) ','Output File (Sink)', 'Token Bucket' )

hold off


figure(2)
plot(time7, content)
title('Exercise 2.3 - Rate 3 - Content and Backlog');
ylabel('Frame Size (Bytes)');
xlabel('Time (Miliseconds)');

hold on

plot(time7, backlog)
legend('Content','Backlog')

hold off




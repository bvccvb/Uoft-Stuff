clc;clear all;

[sequence_number_s, send_time] = textread('send.txt', '%f %f');
[sequence_number_r, receive_time] = textread('rec.txt', '%f %f');

receive_time  = receive_time - send_time(1);
send_time = send_time - send_time(1);
receive_time = receive_time/1000;
send_time = send_time/1000;

packet_size = 4;
max_backlog = -1; %in bytes

for i = 1 : size(receive_time)
    for j = i : size(send_time)
        if send_time(j) >= receive_time(i)
            backlog = (j-i)*packet_size;
            if backlog > max_backlog
                max_backlog = backlog;
                break;
            end
        end
    end
end

max_backlog_bits = max_backlog * 8

%Service Curves

%% ado thd
reg_pw('a54',21, 15, 0,'fa06'); %AN thd

%% Hybrid (Leo)
dec2hex(reg_pw('0', 27, 15, 0, '84e4'));
dec2hex(reg_pw('0', 28, 15, 0, '0f03'));    % % [15:8]: tx_delay_100
dec2hex(reg_pw('0', 27, 15, 0, '84e6'));
dec2hex(reg_pw('0', 28, 15, 0, '0907'));    % % [15:8]: pga_re_100

%% Latest TC10(CY)
dec2hex(reg_pw('0', 27, 15, 0, '84e2'));
dec2hex(reg_pw('0', 28, 15, 0, '0803'));    % % [8:0]: dac_i_sel_wup_1000

reg_pw('a4e',19, 15, 0,'0002'); %AN thd

%clear all;

base_dir='~/home_r3s6/torbjorr/snowflake-out3';
growthrate=[15000000,22000000,33000000];
meltrate=[500,1000,2000];
droprate=[10000,20000,50000];

global i_alpha=1;
global i_beta=2;
global i_rms=3;
global i_speed_avg=4;
global i_vrec_avg=5;
global i_R_max_avg=6;
global i_max=i_R_max_avg;

%[frame_data,frame_last]=mod34_stats_cloud(base_dir...
%	,growthrate(1),meltrate(1),droprate(3));

figure(i_beta,'paperunits','centimeters','papersize',[8 5],'paperposition',[0 0 8 5]);
plot(frame_data(2:end,3)/max(frame_data(:,3)),frame_data(2:end,i_beta),'Linewidth',1);
xlabel('$\tau/\tau_\text{max}$');
ylabel('$\beta$');

[beta_final,ratio,error]=limit_estimate(frame_data(2:end,3),frame_data(2:end,i_beta))

print('/home/torbjorr/Dokument/Skolarbeten/Chalmers/exjobb/local/report/output/betaconv','-dpdflatex');

figure(i_rms);
plot(frame_data(2:end,3)/max(frame_data(:,3)),frame_data(2:end,i_rms),'Linewidth',1);


figure(1)
[nn,xx]=hist_fdr(frame_last(:,1));
semilogy(xx,nn,'Linewidth',1);

figure(2)
plot(frame_last(:,1),frame_last(:,2),'.')

%figure(2,'paperunits','centimeters','papersize',[6.5,4]...
%	,'paperposition',[0 0 6.5 4]);
%plot(frame_last(:,1),frame_last(:,2),'.','markersize',1);
%hold on;
%v_line=linspace(min(frame_last(:,1)),max(frame_last(:,1)),128);
%plot(v_line,frame_data(end,i_alpha).*v_line.^frame_data(end,i_beta),'Linewidth',1);
%clear v_line;
%xlabel('$R_\text{max}$');
%ylabel('$V$');
%hold off
%print('/home/torbjorr/laptop_r4/Dokument/Skolarbeten/Chalmers/exjobb/local/report/output/test','-dpdflatex');


% figure(1);
% plot(frame_data(:,3),frame_data(:,4),'Linewidth',1);
% xlabel('\tau');
% ylabel('N_\text{cloud}');
%
% figure(2);
% plot(frame_data(:,3),frame_data(:,5),'Linewidth',1);
% xlabel('\tau');
% ylabel('N_\text{dropped}');
%

% figure(4)
% [N,x]=hist_fdr(frame_last(:,1));
% semilogy(x,N,'Linewidth',1);
%
% figure(5)
% [N,x]=hist_fdr(frame_last(:,5));
% plot(x,N,'Linewidth',1);
%
% figure(6)
% [N,x]=hist_fdr(frame_last(:,6));
% plot(x,N,'Linewidth',1);
%
% figure(i_speed_avg);
% plot(frame_data(:,3),frame_data(:,i_speed_avg),'Linewidth',1);
% xlabel('\tau');
% ylabel('\left|\vec{v}\right|');
%
% figure(i_vrec_avg)
% plot(frame_data(:,3),frame_data(:,i_vrec_avg),'Linewidth',1);
% xlabel('\tau');
% ylabel('\frac{1}{V}');
%
% figure(i_R_max_avg)
% plot(frame_data(:,3),frame_data(:,i_R_max_avg),'Linewidth',1);
% xlabel('\tau');
% ylabel('R_\text{max}');
%



%
%function mod34_sats_main(base_dir,growthrate,meltrate,droprate)
%	for k=1:length(growthrate)
%		for l=1:length(meltrate)
%			for m=1:length(droprate)
%				mod34_sats(base_dir,growthrate(k),meltrate(l),droprate(m));
%			end
%		end
%	end
%end
%

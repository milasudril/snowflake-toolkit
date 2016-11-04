function sizedistplot2()
	size_pt=0.35136;
	textwidth=418*size_pt/10;
	textheight=595*size_pt/10;
	figwidth=0.75*textwidth;
	figheight=(textheight-4.75)/3;

%	fig=figure('paperunits','centimeters','papersize'...
%		,[figwidth figheight],'paperposition',[0 0 figwidth figheight])

	v2=struct('growthrate',22000000,'meltrate',1000,'droprate',50000);
	values={v2};
	hold on
	for k=1:numel(values)
		data=frame_load('~/home_r3s6/torbjorr/snowflake-out4',values{k}.growthrate...
			,values{k}.meltrate,values{k}.droprate,'frame-dropped'...
			,1024);
		[N,R] = hist_fdr(data(:,1));
		i_nz=find(R<=3);
	%	plot(R(i_nz),N(i_nz),'-','markersize',2,'color',k*[0.2 0.2 0.2],'linewidth',1);
		[R_f,N_f,d]=noneqdist_filter(R(i_nz)',N(i_nz)',1/8);
		semilogy(R_f,N_f,'r','linewidth',1);
	%	plot(R_f,d,'b','linewidth',1);
	%	[C,lambda_tmp,e_rms]=expfit(R',N');
	%	x=linspace(0,max(R(i_nz)),128);
	%	semilogy(x,C*exp(lambda_tmp*x),'--','color',k*[0.2 0.2 0.2]);
	end
	xlabel('$R_\text{max}$');
	ylabel('$C\dd{R_\text{max}}N$');
	hold off
end

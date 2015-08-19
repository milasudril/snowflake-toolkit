function sizedistplot(base_dir,dir_out,filename,prefix,values)
	size_pt=0.35136;
	textwidth=418*size_pt/10;
	textheight=595*size_pt/10;
	figwidth=0.75*textwidth;
	figheight=(textheight-4.75)/3;

	fig=figure('paperunits','centimeters','papersize'...
		,[figwidth figheight],'paperposition',[0 0 figwidth figheight])
	hold on
	for k=1:numel(values)
		data=frame_load(base_dir,values{k}.growthrate...
			,values{k}.meltrate,values{k}.droprate,prefix...
			,1024);
		[N,R] = hist_fdr(data(:,1));
		i_nz=intersect(find(N),find(R<=3));
		size(i_nz)
		semilogy(R(i_nz),N(i_nz),'-','markersize',2,'color',k*[0.2 0.2 0.2]);
		[C,lambda_tmp,e_rms]=expfit(R',N');
		x=linspace(0,max(R(i_nz)),128);
		semilogy(x,C*exp(lambda_tmp*x),'--','color',k*[0.2 0.2 0.2]);
	end
	xlabel('$R_\text{max}$');
	ylabel('$C\dd{R_\text{max}}N$');
	wd=cd(dir_out);
	print(filename,'-dpdflatex');
	delete(fig);
end

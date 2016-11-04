function rvplot(base_dir,prefix,dir_out,filename)
	size_pt=0.35136;
	textwidth=418*size_pt/10;
	textheight=595*size_pt/10;
	figwidth=0.9*textwidth;
	figheight=1.2*(textheight-4.75)/3;

	fig=figure('paperunits','centimeters','papersize'...
		,[figwidth figheight],'paperposition',[0 0 figwidth figheight])
	hold on

	data=frame_load(base_dir,prefix,1024);
	ax=loglog(data(:,1),data(:,2),'.','markersize',0.5);
	[alpha,beta_tmp,e_rms]=powerfit(data(:,1),data(:,2));
	x=linspace(0.3*min(data(:,1)),3*max(data(:,1)),128);
	loglog(x,alpha.*x.^beta_tmp);

	xlabel('$R_\text{max}$');
	ylabel('$V$');
	legend('Data points',sprintf('$V=%.3g {R_\\text{max}}^{%.3g}$',alpha,beta_tmp),'location','northeastoutside');
	legend boxoff
	wd=cd(dir_out);
	wd=cd(dir_out);
	print(filename,'-dpdflatex');
	delete(fig);
end

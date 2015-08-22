function abetaplot(dir_out,filename)
	size_pt=0.35136;
	textwidth=418*size_pt/10;
	textheight=595*size_pt/10;
	figwidth=0.75*textwidth;
	figheight=1.2*(textheight-4.75)/3;

	fig=figure('paperunits','centimeters','papersize'...
		,[figwidth figheight],'paperposition',[0 0 figwidth figheight])
	hold on

	load ../results/mod34_beta_a_1.mat

	ax=plot(tab(:,1),tab(:,2),'-','markersize',0.5);
	hold on
	ax=plot(tab(:,1),tab(:,4),'--','markersize',0.5);

	xlabel('$a$');
	ylabel('$\beta$');
	wd=cd(dir_out);
	print(filename,'-dpdflatex');
	delete(fig);
end

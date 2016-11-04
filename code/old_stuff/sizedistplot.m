function sizedistplot(base_dir,prefix,frame,dir_out,filename)
	size_pt=0.35136;
	textwidth=418*size_pt/10;
	textheight=595*size_pt/10;
	figwidth=0.9*textwidth;
	figheight=1.2*(textheight-4.75)/3;

	fig=figure('paperunits','centimeters','papersize'...
		,[figwidth figheight],'paperposition',[0 0 figwidth figheight])
	hold on

	data=frame_load(base_dir,prefix,frame);
	if size(data,1)>1
		i_1=find(data(:,7)==1);
		r_0=mean(data(i_1,1))
		i_g1=find(data(:,7)>1);
		if size(i_g1,1)>1
			[N,R] = hist_fdr(data(i_g1,1)/r_0);
			[~,i_max]=max(N);
			n_0=sum(N(i_max:end));
			cum=1-cumsum(N(i_max:end))/n_0;
			[C,lambda_tmp,e_rms]=expfit(R(i_max:end)',cum');
			semilogy(R,N,'.','markersize',1.5);
			hold on
			r_int=linspace(0,max(R),128);
			lambda_tmp
			semilogy(r_int,-exp(lambda_tmp*r_int)*lambda_tmp);
		end
	end

	xlabel('$\frac{R_\text{max}}{r_\text{max}}$');
	ylabel('$\dd{N}\left(\frac{R_\text{max}}{r_\text{max}}\right)$');
	legend('Data points',sprintf('Data fit $\\gamma=%.2g$',lambda_tmp),'location','northeastoutside')
	legend boxoff
	wd=cd(dir_out);
	print(filename,'-dpdflatex');
	cd(wd)
	delete(fig);
end

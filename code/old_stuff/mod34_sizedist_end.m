function table=mod34_sizedist_end(base_dir,base_dir_out,name_prefix)
%	table=mod34_stats_beta_cb(33000000,500,10000,base_dir);
	global figs=struct();

	table=mod34_stats_main(@(growthrate,meltrate,droprate)...
		mod34_stats_beta_cb(growthrate,meltrate,droprate,base_dir));

	fig_ids=fieldnames(figs);
	old_dir=cd(base_dir_out);
	for k=1:numel(fig_ids)
		figure( figs.(fig_ids{k}) );
		print(sprintf('%s%s.pdf',name_prefix,fig_ids{k}),'-dpdflatex');
		delete( figs.(fig_ids{k}) );
	end
	cd(old_dir);

	clear all;
end

function table_row=mod34_stats_beta_cb(growthrate,meltrate,droprate,base_dir)
	global figs;
	fig_id=sprintf('m%d_d%d',meltrate,droprate);
	if(~isfield(figs,fig_id))
		size_pt=0.35136;
		textwidth=418*size_pt/10;
		textheight=595*size_pt/10;
		figwidth=0.75*textwidth;
		figheight=(textheight-4.75)/3;

		figs.(fig_id)=figure('paperunits','centimeters','papersize'...
			,[figwidth figheight],'paperposition',[0 0 figwidth figheight]);
		xlabel('$R_\text{max}$');
		ylabel('$C\dd{R_\text{max}}N$');
		ylim([1e-5, 1]);
		set(gca(),'Ytick',[1e-5 1e-4 1e-3 1e-2 1e-1 1]);
	end
	dir=sprintf('%s/%d-%d-%d',base_dir,growthrate,meltrate,droprate)
	data=frame_load(dir,'frame-dropped',1024);
	i_ge1=find(data(:,7)>1);
	[N,R] = hist_fdr(data(i_ge1,1));
	i_nz=intersect(find(N),find(R<=10));
	figure(figs.(fig_id));
	hold on
	semilogy(R(i_nz),N(i_nz));
	hold off

	table_row=[0];
end

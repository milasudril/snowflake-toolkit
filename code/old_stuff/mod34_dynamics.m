function table=mod34_dynamics(base_dir,base_dir_out,name_prefix)
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
		xlabel('$\tau$');
		ylabel('$N_\text{cloud}$');
		ylim([0,5000]);
	end

	frame_data=frame_data_load(base_dir,growthrate,meltrate,droprate);
	figure(figs.(fig_id));
	hold on
	plot(frame_data(:,3),frame_data(:,4),'linewidth',1);
	hold off

	table_row=[0];
end

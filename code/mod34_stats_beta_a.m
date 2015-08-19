function table=mod34_stats_beta_a(base_dir)
%	table=mod34_stats_beta_cb(33000000,500,10000,base_dir);
	table=mod34_stats_main_a(@(a)...
		mod34_stats_beta_cb(a,base_dir));
end

function table_row=mod34_stats_beta_cb(a,base_dir)
	dir=sprintf('%s/%.7g',base_dir,a);
	[tau,N,beta_cloud]=beta_comp(dir,'frame');
	i_find=find(beta_cloud);
	[beta_final_c,ratio_c]=limit_estimate(tau(i_find),beta_cloud(i_find));
	fprintf(2,'%.7g %.7g\n',beta_final_c,ratio_c);
	fflush(2);

	[~,~,beta_dropped]=beta_comp(dir,'frame-dropped');
	i_find=find(beta_dropped);
	[beta_final_d,ratio_d]=limit_estimate(tau(i_find),beta_dropped(i_find));
	fprintf(2,'%.7g %.7g\n',beta_final_d,ratio_d);
	fflush(2);

	table_row=[a,beta_final_c,ratio_c,beta_final_d,ratio_d,max(tau)];
end

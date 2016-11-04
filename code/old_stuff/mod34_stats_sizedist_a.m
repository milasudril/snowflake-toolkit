function table=mod34_stats_sizedist_a(base_dir)
%	table=mod34_stats_beta_cb(33000000,500,10000,base_dir);
	table=mod34_stats_main_a(@(a)...
		mod34_stats_sizedist_cb(a,base_dir));
end

function table_row=mod34_stats_sizedist_cb(a,base_dir)
	dir=sprintf('%s/%.7g',base_dir,a);
	[tau,N,lambda_cloud]=sizedist_comp(dir,'frame');
	i_find=find(lambda_cloud);
	[lambda_final_c,ratio_c]=limit_estimate(tau(i_find),lambda_cloud(i_find));
	fprintf(2,'%.7g %.7g\n',lambda_final_c,ratio_c);
	fflush(2);

	[~,~,lambda_dropped]=sizedist_comp(dir,'frame-dropped');
	i_find=find(lambda_dropped);
	[lambda_final_d,ratio_d]=limit_estimate(tau(i_find),lambda_dropped(i_find));

	fprintf(2,'%.7g %.7g\n',lambda_final_d,ratio_d);
	fflush(2);

	table_row=[a,lambda_final_c,ratio_c,lambda_final_d,ratio_d,max(tau)];
end

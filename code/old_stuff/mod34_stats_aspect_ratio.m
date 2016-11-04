function table=mod34_stats_aspect_ratio(base_dir)
%	table=mod34_stats_beta_cb(33000000,500,10000,base_dir);
	table=mod34_stats_main(@(growthrate,meltrate,droprate)...
		mod34_stats_aspect_ratio_cb(growthrate,meltrate,droprate,base_dir));
end

function table_row=mod34_aspect_ratio_cb(growthrate,meltrate,droprate,base_dir)
	[tau,N,r_xy,r_xz]=mod34_aspect_ratio_comp(base_dir,growthrate,meltrate,droprate,'frame');
	[rxy_final_c,decay_xy_c,ratio_rxy_c,error]=limit_estimate(tau,r_xy);
	[rxz_final_c,decay_xz_c,ratio_rxz_c,error]=limit_estimate(tau,r_xz);

	[~,N,r_xy,r_xz]=mod34_aspect_ratio_comp(base_dir,growthrate,meltrate,droprate,'frame');
	[rxy_final_d,decay_xy_d,ratio_rxy_d,error]=limit_estimate(tau,r_xy);
	[rxz_final_d,decay_xz_d,ratio_rxz_d,error]=limit_estimate(tau,r_xz);

	table_row=[growthrate,meltrate,droprate...
		,beta_final_c,decay_c,ratio_c...
		,beta_final_d,decay_d,ratio_d,max(tau)];
end

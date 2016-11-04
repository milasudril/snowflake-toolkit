function table=mod34_stats_speed(base_dir)
%	table=mod34_stats_beta_cb(33000000,500,10000,base_dir);
	table=mod34_stats_main(@(growthrate,meltrate,droprate)...
		mod34_stats_speed_cb(growthrate,meltrate,droprate,base_dir));
end

function table_row=mod34_stats_speed_cb(growthrate,meltrate,droprate,base_dir)
	[tau,N,v_cloud]=fallspeed_comp(base_dir,growthrate,meltrate,droprate,'frame');
	i_find=find(lambda_cloud);
	[speed_final_c,decay_c,ratio_c,error]=limit_estimate(tau(i_find),lambda_cloud(i_find));

	[~,~,v_dropped]=fallspeed_comp(base_dir,growthrate,meltrate,droprate,'frame');
	i_find=find(lambda_dropped);
	[speed_final_d,decay_d,ratio_d,error]=limit_estimate(tau(i_find),lambda_dropped(i_find));

	table_row=[growthrate,meltrate,droprate...
		,speed_final_c,decay_c,ratio_c...
		,speed_final_d,decay_d,ratio_d,max(tau)];
end

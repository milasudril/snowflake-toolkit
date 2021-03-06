function table=mod34_stats_vr(base_dir)
%	table=mod34_stats_vr_cb(33000000,500,10000,base_dir);
	table=mod34_stats_main(@(growthrate,meltrate,droprate)...
		mod34_stats_vr_cb(growthrate,meltrate,droprate,base_dir));
end

function table_row=mod34_stats_vr_cb(growthrate,meltrate,droprate,base_dir)
	dir=sprintf('%s/%d-%d-%d',base_dir,growthrate,meltrate,droprate);
	[tau,N,vr_cloud]=vr_comp(dir,'frame');
	i_find=find(vr_cloud);
	[vr_final_c,ratio_c]=limit_estimate(tau(i_find),vr_cloud(i_find));
	fprintf(2,'%.7g %.7g\n',vr_final_c,ratio_c);
	fflush(2);

	[~,~,vr_dropped]=vr_comp(dir,'frame-dropped');
	i_find=find(vr_dropped);
	[vr_final_d,ratio_d]=limit_estimate(tau(i_find),vr_dropped(i_find));
	fprintf(2,'%.7g %.7g\n',vr_final_d,ratio_d);
	fflush(2);

	table_row=[growthrate,meltrate,droprate...
		,vr_final_c,ratio_c...
		,vr_final_d,ratio_d,max(tau)];
end

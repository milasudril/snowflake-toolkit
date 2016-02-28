function tab=mod34_stats_rmax(base_dir)
	tab=mod34_stats_main(@(growthrate,meltrate,droprate)...
		mod34_stats_rmax_cb(growthrate,meltrate,droprate,base_dir));
end

function x=mod34_stats_rmax_cb(growthrate,meltrate,droprate,base_dir)
	dir=sprintf('%s/%d-%d-%d',base_dir,growthrate,meltrate,droprate);
	frame_data=frame_data_load(dir);
	sums=[];
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz	Subvols
		data=frame_load(dir,'frame',frame_data(k,1));
		if size(data,1)>0
			i_g1=find(data(:,7)==1);
			sums=[sums;mean(data(:,1))];
		end
		data=frame_load(dir,'frame-dropped',frame_data(k,1));
		if size(data,1)>0
			i_g1=find(data(:,7)==1);
			sums=[sums;mean(data(:,1))];
		end
	end
	x=mean(sums(find(sums>0)));
end



function [frame_data,frame_last,frame_last_dropped]=mod34_stats_cloud(base_dir...
	,growthrate,meltrate,droprate)

	global i_alpha;
	global i_beta;
	global i_rms;
	global i_speed_avg;
	global i_vrec_avg;
	global i_R_max_avg;
	global i_max=i_R_max_avg;

	frame_data=frame_data_load(base_dir,growthrate,meltrate,droprate);
	disp(i_alpha);

	frame_data_comp=zeros(size(frame_data,1),2*i_max);
	for k=1:size(frame_data,1)
	% 	R_max	Volume	Speed	L_x	r_xy	r_xz
		data=frame_load(base_dir,growthrate,meltrate,droprate,'frame'...
  			,frame_data(k,1));
		if ~isempty(data)
			[alpha,beta,e_rms]=powerfit(data(:,1),data(:,2));
			frame_data_comp(k,i_alpha)=alpha;
			frame_data_comp(k,i_beta)=beta;
			frame_data_comp(k,i_rms)=e_rms;
			frame_data_comp(k,i_speed_avg)=mean(data(:,3));
			frame_data_comp(k,i_vrec_avg)=mean(1./data(:,2));
			frame_data_comp(k,i_R_max_avg)=mean(data(:,1));
		end
	end
	frame_last=frame_load(base_dir,growthrate,meltrate,droprate,'frame'...
		,frame_data(end,1));

	i_alpha=i_alpha+size(frame_data,2);
	i_beta=i_beta+size(frame_data,2);
	i_rms=i_rms+size(frame_data,2);
	i_speed_avg=i_speed_avg+size(frame_data,2);
	i_vrec_avg=i_vrec_avg+size(frame_data,2);
	i_R_max_avg=i_R_max_avg+size(frame_data,2);
	i_max=i_R_max_avg;

	frame_data=[frame_data,frame_data_comp];
end

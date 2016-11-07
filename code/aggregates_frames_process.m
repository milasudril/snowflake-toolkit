function aggregates_frames_process(basedir,callback)
% function aggregates_frames_process(basedir,callback)
%
% Processes all frames from an aggregate simulation

	frame_data=aggregates_framedata_load(basedir);
	for k=1:numel(frame_data.Frame)
		frame=aggregates_frame_load(basedir,frame_data.Frame(k));
		callback(basedir,frame);
	end
end

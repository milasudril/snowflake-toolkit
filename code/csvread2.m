function [data,N_rows]=csvread2(filename,delimiter)
	if nargin()<2
		delimiter='\t';
	end
	fd=fopen(filename,'r');
	if fd==-1
		error('Could not open file');
	end
	c=onCleanup(@()fclose(fd));
	headers=strsplit(fgetl(fd),delimiter);

	content=dlmread(filename,delimiter,1,0);
	data=struct();
	N_rows=0;
	for k=1:numel(headers)
		data.(headers{k})=[];
	end
	if ~isempty(content)		
		for k=1:numel(headers)
			data.(headers{k})=content(:,k);
		end
		N_rows=size(content,1);
	end
end

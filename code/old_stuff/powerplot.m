function [A,B]=powerplot(basepath,N)
	A=zeros(N,1);
	B=zeros(N,1);
	for k=1:N-1
		filename=sprintf('%s-%d.txt',basepath,k);
		data=dlmread(filename,'\t');
		[a,b]=powerfit(data(:,1),data(:,2));
		A(k+1)=a;
		B(k+1)=b;
	end
end

function [A]=dexp(x,y,N)
	M=zeros(size(x,1),N);
	for k=1:size(M,2)
		for l=1:size(M,1)
			M(l,k)=exp(-2*k*x(l)/max(x));
		end
	end
	size(M)
	A=M\y;
end

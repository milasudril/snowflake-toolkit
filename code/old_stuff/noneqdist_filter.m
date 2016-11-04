function [X,Y,d]=noneqdist_filter(x,y,t_r)
	omega=2*log(9)/t_r;
	omega=fzero(@(x)solvefun(x,t_r),omega);
	[X,V]=ode_trapetzoid(@(T,Y,k)filterdamped(T,Y,k,y,omega),x,[y(1);0]);
	Y=V(:,1);
	d=V(:,2);
end

function [y]=solvefun(omega,t_r)
	y=1-(omega*t_r + 1)*exp(-omega*t_r) - 0.9;
end

function [V]=filterdamped(t,v,k,x,omega)
%This function generates the derivatives for a second-order critically damped
%filter:
%
%	y'' + 2ωy' + yω^2 = ω^2 x
%
%x is a pp object representing the input data
%
%
%Let v(1)=y, v(2)=y'. Then
%
%	v(2)' = ω^2 x - v(1)ω^2 - 2ωv(2)
%	v(1)' = v(2)

%	x=ppval(ppx,t);
	V=[v(2);omega^2*x(k)-v(1)*omega^2-2*omega*v(2)];
end

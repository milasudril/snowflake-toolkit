function [C,P,c,e_rms]=exppolyfit_tail(X,Y)
%	Fits an exponential model
%	    y=C*(exp(P(x)) + c)
%	where C=+-1, P(x)=P(3)*x^2 + P(2)*x + P(1), and dP/dx < 0 when x>0
%
	C=sign(Y(1)-Y(end));
	Y=Y*C;
	c=Y(end);
	if(c < 0)
		c=min(c,-max(abs(Y)));
		c=c*(1+eps);
	else
		c=min(c,min(abs(Y)));
		c=c*(1-eps);
	end

	P=polyfit(X,log(Y - c),2)';
	x=fminunc(@(X_0)(expfunc(X_0,X,Y)),[P;c]);
	P=x(1:3);
	c=x(4);
	e_rms=sqrt( expfunc([P;c],X,Y)/size(X,1) )/(max(Y) - min(Y));
end

function f=expfunc(X_0,X,Y)
	mod_val=exp(polyval(X_0(1:3),X)) + X_0(4);
	f=sum( (Y - mod_val).^2 );
end

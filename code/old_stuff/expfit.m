function [a,b,e_rms]=expfit(X,Y)
%	Fits an exponential model y=a exp(bx) to measurement data, assuming b<0
	ind_nz=find(Y);
	[y_max,i_ymax]=max(Y);
	if(numel(ind_nz)>1 && abs(y_max - min(Y))>0.5*(y_max+min(Y))*eps)
	%	p=polyfit(X(ind_nz),log(Y(ind_nz)),1);
	%	a=exp(p(2));
	%	b=p(1);
		a=y_max-min(Y);
		b=-1/(max(X));
		x=fminsearch(@(X_0)(expfunc(X_0,X(ind_nz),Y(ind_nz))),[a;b]);
		a=x(1);
		b=x(2);
		e_rms=sqrt( expfunc([a;b],X(ind_nz),Y(ind_nz))/size(X(ind_nz),1) )...
			/(y_max - min(Y));
	else
		a=0;
		b=0;
		e_rms=1e16;
	end
end

function f=expfunc(X_0,X,Y)
	mod_val=X_0(1).*exp(X.*X_0(2));
	f=sum( (Y - mod_val).^2 );
end

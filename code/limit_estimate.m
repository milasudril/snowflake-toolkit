function [limit,predict_ratio]=limit_estimate(X,Y)
	fprintf(2,'\nEstimating limit\n');
	phi=1.5-sqrt(5)/2;
	[X_filt,Y_filt]=noneqdist_filter(X,Y,(max(X)-min(X))/32);
%	X_filt=X;
%	Y_filt=Y;
	limit=0;

	predict_ratio=0;

	for k=1:2
		fit_range=1:floor( (phi^k) *size(X_filt,1));
		if(numel(fit_range)>1)
			[a_e,b_e,c_e,~,i_good]=expfit_tail(X_filt(fit_range),Y_filt(fit_range));
			[a_l,b_l,~]=linfit(X_filt(i_good),Y_filt(i_good));

			x=linspace(min(X),max(X),128);

			check_range=(floor(phi*size(X,1))+1) : size(X,1);

			predict_ratio=max(predict_ratio...
				,norm(a_l*X(check_range)+b_l - Y(check_range))...
					/norm(a_e*exp(b_e*X(check_range))+c_e - Y(check_range)));
		end
	end

	i_0=floor(size(Y,1)*6/8);
	i_end=size(Y,1);
	end_range=i_0:i_end;
	Y_mid=filter([0.5, 0.5],1,Y(end_range),0);
	limit=((Y_mid(2:end))'*diff(X(end_range)))/(X(i_end)-X(i_0));
end

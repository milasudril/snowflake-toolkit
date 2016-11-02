function vols=ice_loader(source)
	input=fileread(source);
	vols={};
	state_current=0;
	tok_current='';
	cmd_current='';
	args_current={};
	for k=1:numel(input)
		ch_in=input(k);
		switch(state_current)
			case 0
				if ~(ch_in>=char(0) && ch_in<=char(32))
					switch(ch_in)
						case '#'
							state_current=1;
						otherwise
							state_current=2;
							tok_current=[tok_current,ch_in];
					end
				end
			case 1
				switch(ch_in)
					case char(10)
						state_current=0;
					case char(13)
						state_current=0;
				end
			case 2
				switch(ch_in)
					case '#'
						state_current=1;
					case '('
						cmd_current=tok_current;
						tok_current='';
					case ','
						args_current=[args_current,{tok_current}];
						tok_current='';
					case ')'
						if ~isempty(tok_current)
							args_current=[args_current,tok_current];
						end
						if strcmp(cmd_current,'volume')
							vols={{}};
						elseif strcmp(cmd_current,'vertex')
							vols{end}.vertices(:,end+1)...
								=[str2double(args_current{1})...
								,str2double(args_current{2})...
								,str2double(args_current{3})];
						elseif strcmp(cmd_current,'face')
							vols{end}.faces(:,end+1)...
								=[uint16(str2double(args_current{1}))...
								,uint16(str2double(args_current{2}))...
								,uint16(str2double(args_current{3}))];
						end
							
						args_current={};
						tok_current='';
					case '\'
						state_current=3;
					case char(10)
						state_current=0;
					case char(13)
						state_current=0;
					otherwise
						tok_current=[tok_current,ch_in];
				end
			case 3
				tok_current=[tok_current,ch_in];
				state_current=2;
		end
	end
end

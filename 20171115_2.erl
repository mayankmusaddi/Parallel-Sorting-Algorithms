-module('20171115_2').
-export([mergesort/3,merge/2,msort/1,main/1,read_list/2, write_list/2]).

merge(A, []) -> A;
merge([], B) -> B;
merge([A|Ca], [B|Cb]) ->
	if
		A < B ->
			[A | merge(Ca, [B|Cb])];
		true ->
			[B | merge([A|Ca], Cb)]
	end.

msort([E]) -> [E];
msort(L) ->
	{A, B} = lists:split(trunc(length(L)/2), L),
  	merge(msort(A), msort(B)).

mergesort(L, PID, 4) -> PID ! msort(L);
mergesort([], PID, D) -> PID ! [];
mergesort([E], PID, D) -> PID ! [E];
mergesort(L, PID, D) ->
	{A, B} = lists:split(trunc(length(L)/2), L),
  	spawn('20171115_2', mergesort, [A,self(),D+1]),
  	spawn('20171115_2', mergesort, [B,self(),D+1]),
	receive S1 -> ok end,
	receive S2 -> ok end,
	K = merge(S1, S2),
  	PID ! K.

read_list(In, L) ->
	case io:fread(In,"","~d") of
		{ok, N} -> read_list(In, lists:append(L,N));
		eof -> L
	end.

write_list(Out, [E]) -> 
	io:format(Out,"~w~n",[E]);
write_list(Out, [N|C]) -> 
	io:format(Out,"~w ",[N]),
	write_list(Out, C).

main([Input_file, Output_file]) ->
	{ok, In} = file:open(Input_file, read),
	{ok, Ln} = file:read_line(In),
	file:close(In),

	Sl = string:tokens(string:trim(Ln)," "),
	L = lists:map( fun(X) -> list_to_integer(X) end, Sl),
	spawn('20171115_2', mergesort, [L,self(),0]),
	receive S -> ok end,
	{ok, Out} = file:open(Output_file, [append]),
	write_list(Out,S),
	file:close(Out).
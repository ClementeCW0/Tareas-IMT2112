rm times.csv
for i in {1..32}
do
	echo 100 > data
	echo 100 >> data
	echo 1000 >> data
	echo "$i" >> data
	./tarea_3
done
python ./strong_scaling.py

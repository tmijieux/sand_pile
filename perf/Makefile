
rerun:
	make -B all

all: omp.pdf

omp.pdf:
	./graph.gp

clean:
	$(RM) omp.pdf

clean2: clean
	$(RM) *.data

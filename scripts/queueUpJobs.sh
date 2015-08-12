executable=/home/aravind/Work/Task-Partitioning/Random-Graph-Generator/main.exe
destinationAPPdir=outputs/tpds15-graphs-r2
destinationDOTdir=outputs/tpds15-graphs-r2-DOT
sleepTime=0.1 # This is by default in seconds. Add m next to it to convert to minutes

node=( 128 256 512 1024 2048 4096 8192 16384 )
indegree=(1)
outdegree=(1 2 3 4 5 6 7 8)
CCR=(0.0001 0.001 0.01 0.1 0.5 1 10)
alpha=(0.1 1.0 10)
beta=(10 25 50 75 95)
gamma=(10 25 50 75 95)

mkdir -p $destinationAPPdir;
mkdir -p $destinationDOTdir;

declare -g arr1
declare -g currentCounter
declare -g numberOfParallelInstances

globalCounter=0
logFile="createdgraphs.log"
echo "" > $logFile

# $1 - $i $topology $2 $3 $4 $6
# $2 - $destinationdir/$algorithmName/"$fileName"."$topology".log
# $3 - $i".partition.SA.""$3"
# $4 - $destinationdir/$algorithmName/partitions/SA.$fileName.$topology
# $5 - $i".partition.GA.""$3"
# $6 - $destinationdir/$algorithmName/partitions/GA.$fileName.$topology
executeConfiguration()
{
	i=$1
	$executable ${arr1[i]} >> $logFile
}

submitJobs()
{
	for n in "${node[@]}"
	do
		for i in "${indegree[@]}"
		do
			for o in "${outdegree[@]}"
			do
				for c in "${CCR[@]}"
				do
					for a in "${alpha[@]}"
					do
						for b in "${beta[@]}"
						do
							for g in "${gamma[@]}"
							do
								arr1[globalCounter]="$n $i $o $c $a $b $g"
								((globalCounter++))
#								echo app-"$n"-"$i"-"$o"-"$c"-"$a"-"$b"-"$g".grf
							done
						done
					done
				done
			done
		done
		echo n - $n done
	done
}

debug_print()
{
	for ((currentCounter=0; currentCounter<globalCounter; currentCounter++))
	do
		echo currentCounter=$currentCounter+++ "${arr1[currentCounter]}"
	done
}

scheduleJobs()
{
	counter=0
	
	for ((currentCounter=0; currentCounter<globalCounter; currentCounter++))
	do
#		echo Directory \#$i
		if [ $counter -lt $numberOfParallelInstances ]
		then
			executeConfiguration $currentCounter&
			echo "Job[$currentCounter] launched!"
			listOfPIDs[$currentCounter]=$!
			((counter++))
		else
#			echo else\; counter=$counter
			breakLoop=0
			while [ $breakLoop -eq 0 ]; do
				counter2=0
				((startJ=currentCounter-numberOfParallelInstances*2))
				if [ $startJ -lt 0 ]
				then
					startJ=0
				fi
				for((j=startJ; j<currentCounter; j++))
				do
					if [ ${listOfPIDs[$j]} -lt 0 ]
					then
						continue
					fi
					kill -0 ${listOfPIDs[$j]} 2>&1 > /dev/null
					retVal=$?
					if [ $retVal -eq 1 ]
					then
#						echo "    counter-$counter listOfPIDs[$j]=${listOfPIDs[$j]}"
						listOfPIDs[$j]=-1
						breakLoop=1
			#			((j--))
						((counter2++))
#						echo "    counter-$counter listOfPIDs[$j]=${listOfPIDs[$j]}"
					fi
				done
				if [ $counter2 -eq 0 ]
				then
					sleep $sleepTime
					continue
				else
					counter=$((counter-counter2))
				fi
				
			done
			executeConfiguration $currentCounter&
			listOfPIDs[$currentCounter]=$!
			((counter++))
			echo "Job[$currentCounter] launched! counter=$counter"
#			echo in else-counter=$i $!
			
		fi
	done
}

clean_up()
{
	echo Clean up called. currentCounter=$currentCounter, numberOfParallelInstances=$numberOfParallelInstances.
	echo Killing Existing processes.
	((startJ=currentCounter-numberOfParallelInstances*2))
	if [ $startJ -lt 0 ]
	then
		startJ=0
	fi
	localCounter=0
	for((j=startJ; j<currentCounter; j++))
	do
		if [ "${listOfPIDs[$j]}" -eq "-1" ]
		then
			continue
		fi
		echo Killing process ${listOfPIDs[$j]}
		kill -9 ${listOfPIDs[$j]}
		((localCounter++))
	done
	echo "$localCounter processes killed. Exiting!"
	exit
}

echo $$ > mypid
trap clean_up SIGALRM SIGUSR1

new_workload()
{
	submitJobs
}

test_workload()
{
	submitJobs
	debug_print
}

main()
{
	echo "Initializing directory variables.."

	echo "Generating workload and queueing up jobs.."
	# new_workload
	test_workload

	echo "Total number of jobs to be executed=$globalCounter. Scheduling now.."

	# echo "Debug stuff.."
	# debug_print

	numberOfParallelInstances=14
	scheduleJobs
	mv app*.grf $destinationAPPdir
	mv app*.dot $destinationDOTdir
}

main
rm mypid

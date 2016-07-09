function run_javadoc()
{
    [[ -d documentation/java ]] || mkdir documentation/java
    if [[ -d documentation/java ]]
    then
	javadoc lasnier.sharedraw org.artisanlogiciel.compression org.artisanlogiciel.graphics org.artisanlogiciel.tomcat -private -cp dist/lib/sharedrawweb-0.0.1.jar -sourcepath java -d documentation/java/
	firefox file://$(pwd)/documentation/java/ &
    else
	echo "[ERROR] documentation/java directory not created" >&2
    fi
}

function launch_webapp()
{
    if [[ ! -d webapp ]]
    then
	echo "[ERROR] webapp direcotry missing !!!" >&2
    else
	pushd webapp
	[[ -x ./gluhack.sh ]] && ./gluhack.sh
	[[ -x ./launch.sh ]] && ./launch.sh
	popd
    fi    
}

function specific_run()
{
    local action=$1

    if [[ "$action" == "javadoc" ]]
    then
	run_javadoc
    elif [[ "$action" == "webapp" ]]
    then
	launch_webapp
    fi
}

specific_menus=(javadoc "Generate_Java_Documentation" webapp "WebApp_Build_And_Run")

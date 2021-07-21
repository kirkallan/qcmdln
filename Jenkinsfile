pipeline {
    agent { label 'virt135' } 
    stages {
        stage('Clone Repo') { 
            steps {
                sh "echo cloning repo" 
            }
        }
        stage('Compile') { 
            steps {
                sh "echo comple the code" 
                sh "cc qcmdln.c"
            }
        }
        stage('Rename') { 
            steps {
                sh "echo rename a.out to qcmdln" 
                sh "mv a.out qcmdln"
            }
        }
    }
}

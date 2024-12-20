#!/usr/bin/env groovy

pipeline {
    agent none

    stages {
        stage('Build and Lint') {
            parallel {
                stage('Lint') {
                    agent { label 'nrf-build' }
                    steps {
                        sh 'git clang-format --style=file --extension=c,h --diff origin/master'
                    }
                }
                stage('nrf5340 example') {
                    agent { label 'nrf-build' }
                    steps {
                        sh '''
                            cd examples/nrf5340
                            west build -b nrf5340dk_nrf5340_cpuapp --pristine
                        '''
                    }
                }
                stage('esp32 example') {
                    agent { label 'esp32-build' }
                    steps {
                        sh '''
                            . $IDF_PATH/export.sh
                            cd examples/esp32
                            idf.py clean build
                        '''
                    }
                }
                stage('stm32 example') {
                    agent { label 'tros-build' }
                    steps {
                        sh '''
                            cd examples/stm32wb55
                            rm -rf build && cmake -B build
                            cmake --build build
                        '''
                    }
                }
                stage('rpi example') {
                    agent { label 'test-pi-3' }
                    steps {
                        sh '''
                            cd examples/rpi
                            rm -rf build && cmake -B build
                            cmake --build build
                        '''
                    }
                }
                stage('rpi spi ip proxy example') {
                    agent { label 'test-pi-3' }
                    steps {
                        sh '''
                            cd examples/rpi_spi_ip_proxy
                            rm -rf build && cmake -B build
                            cmake --build build
                        '''
                    }
                }
            }
        }
        stage('Test RPI Proxy') {
            agent { label 'test-pi-3' }
            environment {
                IP = sh returnStdout: true, script: "echo ${SSH_CONNECTION} |  cut -d ' ' -f3"
                PORT = 45555
            }
            steps {
                sh './examples/rpi_spi_ip_proxy/build/rpi_spi_ip_proxy $PORT &'

                build job: 'rpi-spi-tcp-test',
                    parameters: [string(name: 'RPI_IP', value: IP), string(name: 'RPI_PORT', value: PORT)]
            }
        }
    }
}

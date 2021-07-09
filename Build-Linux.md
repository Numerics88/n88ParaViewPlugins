# Build instructions for Linux

These build instructions are particular to ParaView version 5.9.1 . Check out a different tag of the source code if you want 
to build for another version of ParaView.

There is a very handy Git repository that helps build the plugins. I've tried to do it without this tool, but wasn't 
successful. The method available in the repository is very slick:
```s
https://gitlab.kitware.com/paraview/paraview-plugin-builder/tree/master
```
In general, first build ParaView. Then build the plugins against it. The build has to be *exactly* the same as how 
the distributed binary ParaView is built. That's what makes it so tricky.

Check your mental health before you try and build these plugins...

## Learn more about plugins
The C++ documentation contains dedicated pages:
https://kitware.github.io/paraview-docs/latest/cxx/PluginHowto.html
https://kitware.github.io/paraview-docs/latest/cxx/PluginMigration.html
https://kitware.github.io/paraview-docs/latest/cxx/index.html

The source code contains many plugin examples.
https://gitlab.kitware.com/paraview/paraview/-/tree/master/Examples/Plugins

The wiki used to be the official resources but it is being phased out.

## Build on CentOS Linux 7

The easiest way to do this is to take advantage of Amazon's AWS service. There is information about CentOS
on AWS here:

```s
https://wiki.centos.org/Cloud/AWS
```
Go to aws.amazon.com to launch an EC2 using a pre-defined AMI. To launch it select AIMs from the panel 
on the left, then click "Owned by me" and select "Public images" to choose the correct AMI: 
```s
ami-0686851c4e7b1a8e1
```

Launch a t2.xlarge (4 CPUs, 16 GiB) and configure with default details except including 50GB of data (possibly smaller is OK, but 
it's a pain when running out of disk space during the build). Choose an existing key pair or use an old one if you already created
one for AWS. You might want to name your EC2 instance (e.g. "paraview").

The machine is described here as:

```s
CentOS Linux 7
7.9.2009
us-west-2
ami-0686851c4e7b1a8e1
x86_64
```

Accessing the EC2 instance is possible using SSH and SCP:

```s
ssh -Y -i "/Users/sboyd/.ssh/paraview-aws.pem" centos@ec2-25-88-246-65.us-west-2.compute.amazonaws.com
scp -i "/Users/sboyd/.ssh/paraview-aws.pem" test.txt centos@ec2-25-88-246-65.us-west-2.compute.amazonaws.com:/home/centos
scp -i "/Users/sboyd/.ssh/paraview-aws.pem" centos@ec2-25-88-246-65.us-west-2.compute.amazonaws.com:/home/centos/n88ParaViewPlugins-5.9.1-Linux.tar.bz2 .
```

Set up your machine with some basic tools (GIT, wget):
```s
sudo yum install git
sudo yum install wget
```
You're ready to start building.

## Install paraview-plugin-builder

Go to the home directory of your machine and then grab the plugin builder:
```s
git clone https://gitlab.kitware.com/paraview/paraview-plugin-builder.git
```

You'll need *docker* and there is information about it here:
```s
https://docs.docker.com/engine/install/centos/
```
Install *docker* and configure it:
```s
sudo yum remove docker docker-client docker-client-latest docker-common docker-latest docker-latest-logrotate docker-logrotate docker-engine
sudo yum install -y yum-utils
sudo yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo
sudo yum install docker-ce docker-ce-cli containerd.io
sudo systemctl start docker
```
Make it so you can run as *sudo* while you are user *centos*:
```s
sudo groupadd docker
sudo usermod -aG docker centos
```
Log out and back in for the changes to take effect. Then test that all is working properly:
```s
docker run hello-world
```

## Build ParaView
Now that the AWS machine is ready, start building. 

At this time there is only 5.9.0 available as an option with the plugin builder, but it should be
compatible with all v5.9 builds (until it isn't).
```s
cd ~/paraview-plugin-builder
./run_build_paraview.sh -c 7 v5.9.0
```
Patience...

## Build the plugins

Make a directory to build your plugins and grab the repository:

```s
mkdir -p ~/code/n88ParaViewPlugins/v5.9.1
cd ~/code/n88ParaViewPlugins/v5.9.1
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
git checkout v5.9.1
```
Make sure that N88_BUILD_APPLE is set OFF in 'n88ParaViewPlugins/AIMReader/CMakeLists.txt'

Once everything is in place, you simply build each of the plugins by running the script. I've ordered them below from 
easiest to build to hardest. AIMReader requires n88util and AimIO to build, so it takes more time.
```s
cd /home/centos/paraview-plugin-builder
./run_build_plugin.sh -d /home/centos/code/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/ImageGaussianSmooth v5.9.0
./run_build_plugin.sh -d /home/centos/code/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/N88ModelReader v5.9.0
./run_build_plugin.sh -d /home/centos/code/n88ParaViewPlugins/v5.9.1/n88ParaViewPlugins/AIMReader v5.9.0
```
If successful (I hope!) they will be located in '/home/centos/paraview-plugin-builder/output'. You should see the .so plugins.

## Create an archive for distribution

Move the .so files to a file structure that is standard for n88:

```sh
mkdir -p ~/Numerics88/Plugins/ParaView-5.9
cp /home/centos/paraview-plugin-builder/output/*.so ~/Numerics88/Plugins/ParaView-5.9
cd ~
tar -cvf n88ParaViewPlugins-5.9.1-Linux.tar.bz2 ./Numerics88
```

Then you can extract the archive like this:

```sh
tar xvfj n88ParaViewPlugins-5.9.1-Linux.tar.bz2
```

Test the plugins on a system that has ParaView 5.9 running. If you need to see verbose output as you
load a plugin you can launch ParaView as follows:
```sh
$ env PARAVIEW_LOG_PLUGIN_VERBOSITY=0 ./paraview
```


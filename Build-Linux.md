# Build instructions for Linux

These build instructions are particular to ParaView version 6.0.0. Check out a different tag of the source code if you want
to build for another version of ParaView.

There is a very handy git repository that helps build the plugins. It relies on Docker, or a Docker-alike such podman, and is
very slick.  Be sure to take a peek at the README file.
```s
https://gitlab.kitware.com/paraview/paraview-easy-plugin-builder
```

ParaViewEasyPluginBuilder only supports ParaView versions starting from 5.10.0. If you are building plugins for earlier versions, you will need to use the old builder: https://gitlab.kitware.com/paraview/paraview-plugin-builder

Even if you aren't familiar with Docker, this is the recommended method, but be aware that Docker will need at least 10GB
of storage space, usually in your home directory, to build the needed container.

If you only need Docker for this purpose, see the section below on installing Podman instead of a complete Docker application.

## Setting up Docker on Linux

### **Method 1:** Install Podman (Recommended)

**Note:** Skip this step if you already have Docker installed, or intending to use a complete version of Docker (see the next section).

Podman is a lightweight open-source replacement for Docker.  Once you install it, you can run "docker"
commands just like you would with the real Docker.  It isn't a perfect clone, but usually close enough.

On an ubuntu system install as follows:
```s
sudo apt-get install podman podman-docker
```
On a redhat-based system,
```s
sudo dnf install podman podman-docker
```
The podman-docker package makes a fake "docker" command that emulates docker.  But if you want, you can
use the "podman" command instead, since it takes most of the same options.

> **Important:** Podman does auto-resolve short names like `kitware/...`, which is used in the paraview-easy-plugin-script. You need to add the search registry to Podman's config.
>
> Edit the file:
> ```
> sudo nano /etc/containers/registries.conf
> ```
> Add this under the registries.search section:
> ```
> [registries.search]
> registries = ['docker.io']
> ```

### **Method 2:** Install full Docker

If you prefer using Docker instead of Podman, follow the instructions below.

For more information on installing Docker, visit this site.
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

## Install paraview-easy-plugin-builder

ParaView created scripts that will make building plugins easier on Linux. These scripts use the [plugin-devel Docker image](https://hub.docker.com/r/kitware/paraview_org-plugin-devel) to build plugins with CMake.

Go to the home directory of your machine and then clone the plugin builder:
```s
git clone https://gitlab.kitware.com/paraview/paraview-easy-plugin-builder.git
cd ~/paraview-easy-plugin-builder
```

## (August 2025) v6.0.0 Fix for paraview-easy-plugin-builder
During the time of writing, paraview-easy-plugin-builder has not been updated to work with ParaView v6.0.0. The new Docker image no longer needs SCL to run CMake.

Some libraries that n88ParaViewPlugins need are also no longer present on the new image for v6.0.0, and will need to be installed before building.

Edit the file `run_build_plugin.sh` in *paraview-easy-plugin-builder*
```
...
-- Skip to line 64

# Copy and build the plugin
docker create --name=paraview kitware/paraview_org-plugin-devel:$1 /bin/sh -c "while true; do echo waiting; sleep 1; done"
docker start paraview
# ------- Install required libraries -------------------
docker exec paraview dnf install -y libxslt
# ------------------------------------------------------
docker cp build_plugin.sh paraview:/
docker cp plugin.cmake paraview:/
docker cp plugin.tgz paraview:/
echo "Building the plugin for ParaView ${hashOrTag} using ${nbJobs} jobs ..."
# -- REMOVE "scl enable devtoolset-7 --" in this line --
docker exec paraview sh /build_plugin.sh ${nbJobs}
# ------------------------------------------------------
docker cp paraview:/plugin/build ./output
docker stop paraview
docker rm paraview
echo "Done."

```

## Build the plugins

Make a directory to build your plugins and grab the repository:

```s
mkdir -p ~/code/n88ParaViewPlugins/v6.0.0
cd ~/code/n88ParaViewPlugins/v6.0.0
git clone https://github.com/Numerics88/n88ParaViewPlugins.git
cd n88ParaViewPlugins
```
Make sure that N88_BUILD_PACKAGES is set OFF in 'n88ParaViewPlugins/AIMReader/CMakeLists.txt'

Once everything is in place, you simply build each of the plugins by running the script. I've ordered them below from 
easiest to build to hardest. AIMReader requires n88util and AimIO to build, so it takes more time.
```s
cd ~/paraview-easy-plugin-builder
./run_build_plugin.sh -d ~/code/n88ParaViewPlugins/v6.0.0/n88ParaViewPlugins/ImageGaussianSmooth 6.0.0
./run_build_plugin.sh -d ~/code/n88ParaViewPlugins/v6.0.0/n88ParaViewPlugins/N88ModelReader 6.0.0
./run_build_plugin.sh -d ~/code/n88ParaViewPlugins/v6.0.0/n88ParaViewPlugins/AIMReader 6.0.0
```
If successful, they will be located in '~/paraview-easy-plugin-builder/output'. You should see the .so plugins.

## Create an archive for distribution

Move the .so files to a file structure that is standard for n88:

```sh
mkdir -p ~/Numerics88/Plugins/ParaView-6.0
cp ~/paraview-plugin-builder/output/*.so ~/Numerics88/Plugins/ParaView-6.0
cd ~
tar -cjvf n88ParaViewPlugins-6.0.0-Linux-x86_64.tar.bz2 ./Numerics88
```

Then you can extract the archive like this:

```sh
tar -xjvf n88ParaViewPlugins-6.0.0-Linux-x86_64.tar.bz2
```

Test the plugins on a system that has ParaView 6.0.0 running. If you need to see verbose output as you
load a plugin you can launch ParaView as follows:
```sh
$ env PARAVIEW_LOG_PLUGIN_VERBOSITY=0 ./paraview
```


---

## Optional: Building on Cloud or CI

> **Note:** This section is optional and should only be used if you don't have access to a Linux system for building. Using the EC2 instance specified in this section will also incur charges.
> 
> Additionally, I highly recommend looking into using [GitHub runners](https://docs.github.com/en/actions/concepts/runners/github-hosted-runners) to build the plugins instead of an EC2 instance. ParaView created an [example](https://github.com/Kitware/paraview-ci-example/) on building plugins using Github Actions.

This section is out-of-date, since CentOS 7 has expired, but the same formula should mostly work with
Amazon Linux 2 on Amazon's AWS service.

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
scp -i "/Users/sboyd/.ssh/paraview-aws.pem" centos@ec2-25-88-246-65.us-west-2.compute.amazonaws.com:/home/centos/n88ParaViewPlugins-6.0.0-Linux.tar.bz2 .
```

Set up your machine with some basic tools (GIT, wget):
```s
sudo yum install git
sudo yum install wget
```
You're ready to start building.

## Learn more about plugins

The C++ documentation contains dedicated pages:
- https://www.paraview.org/paraview-docs/nightly/cxx/PluginHowto.html
- https://www.paraview.org/paraview-docs/nightly/cxx/PluginMigration.html
- https://www.paraview.org/paraview-docs/nightly/cxx/index.html

The source code contains many plugin examples.
- https://gitlab.kitware.com/paraview/paraview/-/tree/master/Examples/Plugins

Note that any documentation on the ParaView wiki is out of date.
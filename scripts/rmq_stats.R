library(ggplot2)
library(grid)
library(scales)
library(stringr)
library(plyr)


theme_complete_bw <- function(base_size = 12, base_family = "") {
  theme(
    line =               element_line(colour = "black", size = 0.5, linetype = 1,
                            lineend = "butt"),
    rect =               element_rect(fill = "white", colour = "black", size = 0.5, linetype = 1),
    text =               element_text(family = base_family, face = "plain",
                            colour = "black", size = base_size,
                            hjust = 0.5, vjust = 0.5, angle = 0, lineheight = 0.9),
    axis.text =          element_text(size = rel(0.8), colour = "grey50"),
    strip.text =         element_text(size = base_size * 0.7),
    axis.line =          element_blank(),
    axis.text.x =        element_text(size = base_size * 0.6 , lineheight = 0.9, angle = 0, colour = "black", vjust = 1),
    axis.text.y =        element_text(size = base_size * 0.7, lineheight = 0.9, colour = "black", hjust = 1),
    axis.ticks =         element_line(colour = "black"),
    axis.title.x =       element_blank(),
    axis.title.y =       element_text(size = base_size * 0.9, angle = 90, vjust = 0.5),
    axis.ticks.length =  unit(0.15, "cm"),
    #axis.ticks.margin =  unit(0.1, "cm"),

    legend.background =  element_blank(),
    legend.margin =      unit(0.1, "cm"),
    legend.key.height =  unit(0.5, "cm"),
    legend.key.width =   unit(0.5, "cm"),
    legend.text =        element_text(size = rel(0.8)),
    legend.text.align =  NULL,
    legend.title =       element_blank(),
    legend.title.align = NULL,
    legend.direction =   "horizontal",
    legend.justification = "center",
    legend.box =         NULL,
    legend.position =   "bottom",

    #panel.background =   element_rect(fill = NA, colour = "grey", size = 1.3),
    panel.background =   element_rect(fill = NA, colour = "grey", size = 1.3),
    panel.border =       element_blank(),
    panel.grid.major =   element_line(colour = "grey90", size = 0.7),
    panel.grid.minor =   element_line(colour = "grey90", size = 0.3),
    panel.margin =       unit(0.1, "lines"),

    strip.background =   element_rect(fill = NA, colour = NA),
    strip.text.x =       element_text(colour = "black", size = base_size * 0.8),
    strip.text.y =       element_text(colour = "black", size = base_size * 0.8, angle = -90),

    plot.background =    element_rect(colour = NA, fill = "white"),
    plot.title =         element_text(size = base_size * 1.2),
    plot.margin=         unit(c(3,3,3,3),"mm"),
    complete = TRUE
  )
}

query_plot <- function(d, title="Title", thres=4.0, aes_plot = aes(factor(Range),Time)) {
  d$Time <- as.numeric(as.character(d$Time))
  d$Range <- as.numeric(as.character(d$Range))
  d <- subset(d,d$Time <= thres)
  d <- subset(d,d$Algo != "RMQ_SDSL_BP")
  d <- subset(d,d$Algo != "RMQ_SDSL_SCT")

  plot <- ggplot(d,aes_plot) + ggtitle(title)
  plot <- plot + geom_boxplot(aes(fill = factor(Algo)), outlier.size = 1)
  plot <- plot + ylab("Time [µs]")
  plot <- plot + xlab("N")
  plot <- plot + theme_complete_bw()
  print(plot)
}

query_facet_plot <- function(d, title="Title", thres=4.0, aes_plot = aes(factor(Range),Time)) {
  d$Time <- as.numeric(as.character(d$Time))
  d$Range <- as.numeric(as.character(d$Range))
  d <- subset(d,d$Time <= thres)

  plot <- ggplot(d,aes_plot) + ggtitle(title)
  plot <- plot + geom_boxplot(aes(fill=factor(Algo)),outlier.size = 0.25) + facet_grid(~ Scan)
  plot <- plot + ylab("Time [µs]")
  plot <- plot + xlab("N")
  plot <- plot + theme_complete_bw()
  print(plot)
}

bpe_plot <- function(d, title="Title", aes_plot = aes(factor(d$N),d$BPE)) {
  d$BPE <- as.numeric(as.character(d$BPE))
  d$Algo  <- revalue(d$Algo, c("RMQ_FERRADA"="FERRADA","RMQ_SDSL_BP"="SDSL-BP","RMQ_SDSL_SCT"="SDSL-SCT","RMQ_SDSL_BP_FAST_1024"="SDSL-BP-FAST-1024","RMQ_SDSL_BP_FAST_4096"="SDSL-BP-FAST-4096"))
  
  plot <- ggplot(d,aes(factor(d$N),d$BPE,group=d$Algo,label=round(d$BPE,digits=3))) + ggtitle(title)
  plot <- plot + geom_line(aes(colour=Algo)) + geom_text(vjust=0, check_overlap=TRUE)
  plot <- plot + scale_y_continuous(name = "Bits per Element")
  plot <- plot + theme_complete_bw()
  print(plot)
}

construction_plot <- function(d, title="Title") {
  d$ConstructTime <- as.numeric(as.character(d$ConstructTime))
  d$Algo  <- revalue(d$Algo, c("RMQ_FERRADA"="FERRADA","RMQ_SDSL_BP"="SDSL-BP","RMQ_SDSL_SCT"="SDSL-SCT","RMQ_SDSL_BP_FAST_1024"="SDSL-BP-FAST-1024","RMQ_SDSL_BP_FAST_4096"="SDSL-BP-FAST-4096"))
  
  plot <- ggplot(d,aes(factor(d$N),d$ConstructTime,group=factor(Algo))) + ggtitle(title)
  plot <- plot + geom_line(aes(colour=Algo))
  plot <- plot + scale_y_continuous(name = "Construction Time [s]")
  plot <- plot + theme_complete_bw()
  print(plot)
}

#==========Experiment===========#
experiment_dir="/home/theuer/Dokumente/rmq/experiments/results/"
date="2016-12-18"
seq_type="random"
max_length="10"
delta="0"
tmp <- cbind(date,"rmq_experiment",seq_type,max_length,delta)
experiment <- str_c(tmp,collapse='_');
experiment <- paste(experiment_dir,experiment,sep="")

query <- read.csv2(paste(experiment,"/query_result.csv",sep=""),sep=",",header=TRUE)
query$Time <- as.numeric(as.character(query$Time))
query$Range <- as.numeric(as.character(query$Range))
query$N <- as.numeric(as.character(query$N))
query$Scan <- as.numeric(as.character(query$Scan))

min_n = log10(min(query$N))
max_n = log10(max(query$N))

for (n in  (min_n:max_n)) {
  query_sub <- subset(query,query$N == 10^n)
  tmp_title <- cbind("Sequence length N=10^",n," (",seq_type," values) and increasing query ranges");
  query_plot(query_sub, title = str_c(tmp_title,collapse=""),thres=5)
}

c <- read.csv2(paste(experiment,"/construct_result.csv",sep=""),sep=",",header=TRUE)
bpe_plot(c, title = "Bits per Element with increasing sequence length")
#construction_plot(d,title = "Construction time for increasing N")

#e <- read.csv2("/home/theuer/Dokumente/rmq/experiments/results/experiment2/query_result10^10.csv",sep=",",header=TRUE)
#e$Time <- as.numeric(as.character(e$Time))
#e$Range <- as.numeric(as.character(e$Range))
#e$N <- as.numeric(as.character(e$N))
#query_plot(e, title = "Sequence length N=10^10 (random values) and increasing query range")

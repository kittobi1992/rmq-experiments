library(ggplot2)
library(grid)
library(scales)
library(stringr)
library(plyr)
library(dplyr)
library(reshape2)

theme_complete_bw <- function(base_size = 12, base_family = "") {
  theme(
    line =               element_line(colour = "black", size = 0.5, linetype = 1,
                            lineend = "butt"),
    rect =               element_rect(fill = "white", colour = "black", size = 0.5, linetype = 1),
    text =               element_text(family = base_family, face = "plain",
                            colour = "black", size = base_size,
                            hjust = 0.5, vjust = 0.5, angle = 0, lineheight = 0.9,
                            margin = margin(), debug = FALSE),
    axis.text =          element_text(size = rel(0.8), colour = "grey50"),
    strip.text =         element_text(size = base_size * 0.7),
    axis.line =          element_blank(),
    axis.text.x =        element_blank(),
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

    plot.background =    element_rect(colour = NA, fill = "white"),
    plot.title =         element_text(size = base_size * 1.2),
    plot.margin=         unit(c(3,3,3,3),"mm"),
    complete = TRUE
  )
}


lcp_plot <- function(lcp, title="") {
  
  plot <- ggplot(data=lcp,aes(x=Algo,y=Time,fill=Algo,label=round(Time,digits=2))) + geom_bar(stat="identity")
  plot <- plot + facet_wrap(~ Benchmark, scales="free") + geom_text(vjust=-0.075, check_overlap=TRUE)
  plot <- plot + ylab("Time [s]")
  #plot <- plot + xlab("Range")
  plot <- plot + theme_complete_bw()
  print(plot)
}

#==========Experiment===========#
experiment_dir="C:/Users/tobia/Documents/home/theuer/rmq-experiments/results/"
date="2017-04-08"
tmp <- cbind(date,"lcp_experiment")
experiment <- str_c(tmp,collapse='_');
experiment <- paste(experiment_dir,experiment,sep="")

lcp <- read.csv2(paste(experiment,"/lcp_result.csv",sep=""),sep=",",header=TRUE)
lcp <- subset(lcp, lcp$Benchmark != "text_pitches.sdsl")
lcp$Time <- as.numeric(as.character(lcp$Time))
lcp_plot(lcp,title="Suffix-Tree-Traversion on Pizza&Chilli Benchmarks")

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
    axis.text.x =        element_text(size = base_size * 0.6 , lineheight = 0.9, angle = 0, colour = "black", vjust = 1),
    axis.text.y =        element_text(size = base_size * 0.7, lineheight = 0.9, colour = "black", hjust = 1),
    axis.ticks =         element_line(colour = "black"),
    axis.title.x =       element_blank(),
    axis.title.y =       element_text(size = base_size * 0.9, angle = 90, vjust = 0.5),
    axis.ticks.length =  unit(0.15, "cm"),
    #axis.ticks.margin =  unit(0.1, "cm"),
    
    legend.background =  element_blank(),
    legend.margin =      unit(0.25, "cm"),
    legend.key.height =  unit(1.5, "cm"),
    legend.key.width =   unit(1.5, "cm"),
    legend.text =        element_text(size = rel(1.25)),
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


query_range_cache_misses_plot <- function(d, title="") {
  df <- data.frame(Algo=factor(d$Algo),Range=factor(d$Range),CacheMisses=d$CacheMisses)
  plot <- ggplot(df,aes(x=Range, y=CacheMisses, fill=Algo, label=round(d$CacheMisses,digits=3))) + ggtitle(title)
  plot <- plot + geom_bar(stat="identity", position = position_dodge())
  plot <- plot + ylab("Cache Misses")
  plot <- plot + xlab("N")
  plot <- plot + theme_complete_bw()
  print(plot)
}

query_range_cache_references_plot <- function(d, title="") {
  df <- data.frame(Algo=factor(d$Algo),Range=factor(d$Range),CacheReferences=d$CacheReferences)
  plot <- ggplot(df,aes(x=Range, y=CacheReferences, fill=Algo, label=round(d$CacheReferences,digits=3))) + ggtitle(title)
  plot <- plot + geom_bar(stat="identity", position = position_dodge())
  plot <- plot + ylab("Cache References")
  plot <- plot + xlab("N")
  plot <- plot + theme_complete_bw()
  print(plot)
}


#==========Experiment===========#
experiment_dir="/home/theuer/Dokumente/rmq-experiments/results/"
date="2017-04-10"
seq_type="random"
max_length="8"
delta="0"
tmp <- cbind(date,"rmq_experiment",seq_type,max_length,delta,"with_cache_misses")
experiment <- str_c(tmp,collapse='_');
experiment <- paste(experiment_dir,experiment,sep="")

cache_miss <- read.csv2(paste(experiment,"/cache_miss_result.csv",sep=""),sep=",",header=TRUE)
cache_miss$Range <- as.numeric(as.character(cache_miss$Range))
cache_miss$N <- as.numeric(as.character(cache_miss$N))
cache_miss$MissRatio <- as.numeric(as.character(cache_miss$MissRatio))
cache_miss$CacheMisses <- as.numeric(as.character(cache_miss$CacheMisses))
cache_miss$CacheReferences <- as.numeric(as.character(cache_miss$CacheReferences))

cache_miss <- subset(cache_miss, cache_miss$Algo != "RMQ_SDSL_REC_OLD_1024_2")
cache_miss$Algo  <- revalue(cache_miss$Algo, c("RMQ_FERRADA"="BP-Ferrada","RMQ_SDSL_SCT"="SDSL-OLD","RMQ_SUCCINCT"="SUCCINCT","RMQ_SDSL_REC_NEW_1024_2"="NEWRMQ"))

min_n = log10(min(cache_miss$N))
max_n = log10(max(cache_miss$N))

for (n in  (min_n:max_n)) {
  cache_miss_sub <- subset(cache_miss,cache_miss$N == 10^n)
  query_range_cache_misses_plot(cache_miss_sub, paste("Cache Misses for N=10^",n,sep=""))
  query_range_cache_references_plot(cache_miss_sub, paste("Cache References for N=10^",n,sep=""))
}

